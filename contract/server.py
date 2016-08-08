from ethereum import _solidity as solidity
import subprocess
import os, sys
import json

"""
personal.unlockAccount('0xD6B08EAf2C60887B8BB0f013643B749f8C2fA1Ff','',0); 
personal.unlockAccount('0xD2ABD421BE57057520c22EcC65c30FBbE77e3dE8','',0); 
personal.unlockAccount('0x3b49882D180317ADd06e92562C0e7A004c3d28C3','',0);
"""

ALICE_ADDRESS = '0xD6B08EAf2C60887B8BB0f013643B749f8C2fA1Ff'
BOB_ADDRESS = '0xD2ABD421BE57057520c22EcC65c30FBbE77e3dE8'

FILES = [
	"Layer_conv1_conv_0_hashtree.txt",
	"Layer_conv1_bias_1_hashtree.txt",
	"Layer_conv1_relu_2_hashtree.txt",
	"Layer_pool1_pool_3_hashtree.txt",

	"Layer_conv2_conv_4_hashtree.txt",
	"Layer_conv2_bias_5_hashtree.txt",
	"Layer_conv2_relu_6_hashtree.txt",
	"Layer_pool2_pool_7_hashtree.txt",

	"Layer_fc1_matmul_9_hashtree.txt",
	"Layer_fc1_bias_10_hashtree.txt",
	"Layer_fc1_relu_11_hashtree.txt",

	"Layer_fc2_matmul_12_hashtree.txt",
	"Layer_fc2_bias_13_hashtree.txt",
	"Layer_fc2_relu_14_hashtree.txt"
]

image_input = \
'70c06dd243330a625cfe7495c41cbae41eb14a58e90b3a34ce6d2a0f947ad6c0'

hash_to_bytes32 = lambda x : x.decode('hex')

#def escape_all(s):
#    return "'" + ''.join('\\x%02x' % ord(c) for c in s) + "'"
def escape_all(s):
    return "'0x" + s.encode('hex') + "'"

def quote_str_array(a):
    return '[' + ','.join(map(escape_all,a)) + ']'


class Base:
    def _geth(self, commands):
        cmd = """geth --exec "var name='{name}'; var address='{address}'; loadScript('./fetchContractInfo.js'); {commands}" --testnet attach ipc:///home/amiller/.ethereum/testnet/geth.ipc""".format(
            address=self.a,
            name=self.name,
            commands=commands)
        #print cmd
        return subprocess.check_output(cmd, shell='true')

class Client(Base):
    def __init__(self):
        self.name = 'Customer'
        self.a = '0x3b49882D180317ADd06e92562C0e7A004c3d28C3'

    def determineDirection(self):
        s = self._geth("""instance.determineDirection({from: '%s', gas: 1000000})""" % (self.a,))
        return s

    def verifyReLU(self, input_data, alice_result, bob_result):
        s = self._geth("""instance.verifyReLU(%s, %s, %s, {from: '%s', gas: 1000000})""" % (self.a,))
        return s

    def status(self):
        #print self.name, 'fetching status'
        s = self._geth("""var a1 = '0x{alice}'; var a2 = '0x{bob}'; [instance.servers(a1), instance.servers(a2), instance.getAt(), instance.getBound()]""".format(alice=ALICE_ADDRESS, bob=BOB_ADDRESS))
        ((addr1, predict1, lHash1, rHash1), 
         (addr2, predict2, lHash2, rHash2),
         layer, (lBound, rBound)) = json.loads(s)
        #s = self._geth('')
        #layer = json.loads(s)
        #s = self._geth('')
        #lBound, rBound = json.loads(s)
        return dict(addr1=addr1, predict1=predict1, 
                    lHash1=lHash1, rHash1=rHash1, 
                    addr2=addr2, predict2=predict2, 
                    lHash2=lHash2, rHash2=rHash2, 
                    layer=layer, lBound=lBound, rBound=rBound)

    def run(self):
        status = self.status()
        while True:
            lBound = status['lBound']
            rBound = status['rBound']
            # Proceed to leaf
            if lBound == rBound: break

            while True:
                status = self.status()
                if (status['lHash1'] != status['rHash1'] and
                    status['lHash2'] != status['rHash2']):
                    break

            print 'Customer determine direction'
            self.determineDirection()

            # Poll to wait until determineDirection has taken effect
            while True:
                status = self.status()
                if lBound != status['lBound']: break
                if rBound != status['rBound']: break

        print 'Reached the end!'

class Server(Base):
    def __init__(self, name, a, prefix, predict_path, root_path):
        self.prefix = prefix
        self.name = name
        self.a = a
        with open(predict_path, "r") as input_file:
            self.predict = int(input_file.readline())
            with open(root_path, "r") as input_file:
                self.root_hash = map(hash_to_bytes32, input_file.readline().split())
                self.tree_size = map(int, input_file.readline().split())

    def status(self):
        #print self.name, 'fetching status'
        s = self._geth('[instance.servers(address), instance.getAt(), instance.getBound()]')
        ((addr, predict, lHash, rHash), layer, (lBound, rBound)) = json.loads(s)
        #s = self._geth('')
        #layer = json.loads(s)
        #s = self._geth('')
        #lBound, rBound = json.loads(s)
        return dict(addr=addr, predict=predict, lHash=lHash, rHash=rHash, layer=layer, lBound=lBound, rBound=rBound)

    def submitPrediction(self):
        print self.name, 'submitPrediction()'
        # Call geth on command line
        s = self._geth("""var root_hash=%s; var tree_size=%s; instance.submitPrediction(%d, root_hash, tree_size, {from: '%s', gas: 1000000})""" % 
                       (quote_str_array(self.root_hash),
                        self.tree_size, 
                        self.predict, 
                        self.a))
        print s
        return s
        # contract.submitPrediction(self.predict, self.root_hash, self.tree_size, sender = self.k)
        
    def loadHashTree(self, layer):
        filename = os.path.join(self.prefix, FILES[layer])
        with open(filename, "r") as input_file:
            self.hash_tree = map(hash_to_bytes32, input_file.readline().split())
            
    def submitHash(self, l, r):
        print self.name, 'submitHash(%d, %d)' % (l, r)
        def get_id(l, r):
            return (l + r) | int(l != r)

        m = (l + r) >> 1
        l_id = get_id(l, m)
        r_id = get_id(m + 1, r)
        # print self.hash_tree[l_id], self.hash_tree[r_id]
        # Call geth
        s = self._geth("""instance.submitHash(%s, %s, {from: '%s', gas:1000000});""" % (escape_all(self.hash_tree[l_id]), escape_all(self.hash_tree[r_id]), self.a))
        return s
        #contract.submitHash(self.hash_tree[l_id], self.hash_tree[r_id], sender = self.k)

    def get_position(self, layer, position):
        if layer == 0:
            filename = "Layer_data_input_-1_output.txt"
        else:
            filename = FILES[layer].replace('hashtree', 'output')
        with open(os.path.join(self.prefix, filename), "r") as input_file:
            data = map(int, input_file.read().split())
        return data[position]

    def submitPathHash(self, layer, pos):
        my_input = self.get_position(layer, pos)
        my_output = self.get_position(layer + 1, pos)

        def get_id(l, r):
            return (l + r) | int(l != r)
            
        l, r = 0, self.tree_size[layer] - 1
        l_hash = []
        r_hash = []
        parent_hash = []
        while l != r:
            m = (l + r) >> 1
            l_id = get_id(l, m)
            r_id = get_id(m + 1, r)
            parent_hash.append(self.hash_tree[get_id(l, r)])
            l_hash.append(self.hash_tree[l_id])
            r_hash.append(self.hash_tree[r_id])
            if pos <= m:
                r = m
            else:
                l = m + 1
                
        # TODO Call geth
        s = self._geth("""instance.submitPathHash(%s, %s, %s, %s, %s, {from: '%s', gas:1000000});""" % 
                       (my_input,
                        my_output,
                        quote_str_array(parent_hash),
                        quote_str_array(l_hash),
                        quote_str_array(r_hash),
                        self.a))
        return s
        #return contract.submitPathHash(parent_hash, l_hash, r_hash, 
        #                               sender = self.k)

    def run(self):
        # Server
        # 1. submitPrediction
        # 2. wait for lbound!=-1
        # 3. loadHashTree(layer, prefix)
        # 4. finally, submitPathhash

        status = self.status()
        self.loadHashTree(status['layer'])
        while True:
            lBound = status['lBound']
            rBound = status['rBound']
            # Proceed to leaf
            if lBound == rBound: break

            print "[l, r] = [%d, %d]" % (lBound, rBound)
            self.submitHash(lBound, rBound)

            # Poll until the status is different 
            while True:
                status = self.status()
                if status['lBound'] != lBound or status['rBound'] != rBound:
                    print self.name, 'status changed'
                    break
                
        # Reached leaf
        print self.name, 'reached leaf', lBound
        self.submitPathHash(status['layer']-1, lBound)


def main():
    name = sys.argv[1]
    global prefix
    global server
    global client

    if name == 'Alice':
        address = ALICE_ADDRESS
        predict_path = "../good/predict.txt"
        root_path = "../good/hashtree_roots.txt"
        prefix='../good'

    elif name == 'Bob':
        address = BOB_ADDRESS
        predict_path = "../bad/predict.txt"
        root_path = "../bad/hashtree_roots.txt"
        prefix='../bad'

    if name in ('Alice', 'Bob'):
        server = Server(name, address, prefix, predict_path, root_path)
        #server.submitPrediction()
        server.run()

    elif name == 'Customer':
        client = Client()
        client.run()
        

    
if __name__ == '__main__':
    main()
