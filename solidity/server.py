from ethereum import _solidity as solidity
import subprocess
import os, sys
import json

#from eth_rpc_client import Client
#client = Client(host="127.0.0.1", port="8545")

FILES = [
	"Layer_conv1_conv_0_hashtree.txt",
	"Layer_conv1_bias_1_hashtree.txt",
	"Layer_conv1_relu_2_hashtree.txt",
	"Layer_pool1_pool_3_hashtree.txt",

	"Layer_conv2_conv_4_hashtree.txt",
	"Layer_conv2_bias_5_hashtree.txt",
	"Layer_conv2_relu_6_hashtree.txt",
	"Layer_pool2_pool_7_hashtree.txt",

	"Layer_fc1_matmul_8_hashtree.txt",
	"Layer_fc1_bias_9_hashtree.txt",
	"Layer_fc1_relu_10_hashtree.txt",

	"Layer_fc2_matmul_8_hashtree.txt",
	"Layer_fc2_bias_9_hashtree.txt",
	"Layer_fc2_relu_10_hashtree.txt"
]

image_input = '70c06dd243330a625cfe7495c41cbae41eb14a58e90b3a34ce6d2a0f947ad6c0'

hash_to_bytes32 = lambda x : x.decode('hex')

#def escape_all(s):
#    return "'" + ''.join('\\x%02x' % ord(c) for c in s) + "'"
def escape_all(s):
    return "'0x" + s.encode('hex') + "'"

def quote_str_array(a):
    return '[' + ','.join(map(escape_all,a)) + ']'


class Base:
    def _geth(self, commands):
        cmd = """geth --exec "var name='{name}'; var address='{address}'; personal.unlockAccount('{address}',''); loadScript('./fetchContractInfo.js'); {commands}" --testnet attach ipc:///home/amiller/.ethereum/testnet/geth.ipc""".format(
            address=self.a,
            name=self.name,
            commands=commands)
        #print cmd
        return subprocess.check_output(cmd, shell='true')

    def status(self):
        print self.name, 'fetching status'
        s = self._geth('instance.servers(address)')
        (addr, predict, lHash, rHash) = json.loads(s)
        s = self._geth('instance.getAt()')
        layer = json.loads(s)
        s = self._geth('instance.getBound()')
        lBound, rBound = json.loads(s)
        return dict(addr=addr, predict=predict, lHash=lHash, rHash=rHash, layer=layer, lBound=lBound, rBound=rBound)

class Client(Base):
    def __init__(self):
        self.name = 'Customer'
        self.address = '0x3b49882D180317ADd06e92562C0e7A004c3d28C3'

    def determineDirection(self):
        s = self._geth("""instance.determineDirection({from: '%s', gas: 1000000})""" % (self.a,))
        return s

    def verifyReLU(self, input_data, alice_result, bob_result):
        s = self._geth("""instance.verifyReLU(%s, %s, %s, {from: '%s', gas: 1000000})""" % (self.a,))
        return s

class Server(Base):
    def __init__(self, name, a, predict_path, root_path):
        self.name = name
        self.a = a
        with open(predict_path, "r") as input_file:
            self.predict = int(input_file.readline())
            with open(root_path, "r") as input_file:
                self.root_hash = map(hash_to_bytes32, input_file.readline().split())
                self.tree_size = map(int, input_file.readline().split())

    def submitPrediction(self):
        print self.name, 'submitPrediction()'
        # Call geth on command line
        s = self._geth("""var root_hash=%s; var tree_size=%s; instance.submitPrediction(%d, root_hash, tree_size, {from: '%s', gas: 1000000})""" % 
                       (quote_str_array(self.root_hash),
                        self.tree_size, 
                        self.predict, 
                        self.a))
        return s
        # contract.submitPrediction(self.predict, self.root_hash, self.tree_size, sender = self.k)
        
    def loadHashTree(self, layer, prefix):
        filename = os.path.join(prefix, FILES[layer])
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

    def submitPathHash(self, layer, pos):
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
        s = self._geth("""instance.submitPathHash(%s, %s, %s, {from: '%s', gas:1000000});""" % 
                       (quote_str_array(parent_hash),
                        quote_str_array(l_hash),
                        quote_str_array(r_hash),
                        self.a))
        return s
        #return contract.submitPathHash(parent_hash, l_hash, r_hash, 
        #                               sender = self.k)


def main():
    name = sys.argv[1]
    global prefix
    if name == 'Alice':
        address = '0xD6B08EAf2C60887B8BB0f013643B749f8C2fA1Ff'
        predict_path = "../good/predict.txt"
        root_path = "../good/hashtree_roots.txt"
        prefix='../good'
    elif name == 'Bob':
        address = '0xD2ABD421BE57057520c22EcC65c30FBbE77e3dE8'
        predict_path = "../bad/predict.txt"
        root_path = "../bad/hashtree_roots.txt"
        prefix='../bad'

    global server
    server = Server(name, address, predict_path, root_path)

    # Server
    # 1. submitPrediction
    # 2. wait for lbound!=-1
    # 3. loadHashTree(layer, prefix)
    # 4. finally, submitPathhash

    status = server.status()
    server.loadHashTree(status['layer'], prefix)
    while True:
        lBound = status['lBound']
        rBound = status['rBound']
        # Proceed to leaf
        if lBound == rBound: break

	print "[l, r] = [%d, %d]" % (lBound, rBound)
	server.submitHash(lBound, rBound)

        # Poll until the status is different 
        while True:
            status = server.status()
            if status['lBound'] != lBound or status['rBound'] != rBound:
                print name, 'status changed'
                break

    # Reached leaf
    print name, 'reached leaf', lBound
    server.submitPathHash(input, status['layer']-1, lBound)
    
if __name__ == '__main__':
    main()
