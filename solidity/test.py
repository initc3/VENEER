
from ethereum import _solidity as solidity
from ethereum import tester
from ethereum.tester import state, ABIContract
from ethereum._solidity import get_solidity, compile_file
import os, sys

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

tester_state = state()
contract = tester_state.abi_contract(open("./test.sol").read(), constructor_parameters = (tester.a1, tester.a2, 
'70c06dd243330a625cfe7495c41cbae41eb14a58e90b3a34ce6d2a0f947ad6c0'.decode('hex')), language = "solidity")

print "a0 =", type(tester.a0), str(tester.a0.encode("hex")), type(tester.k0), str(tester.k0.encode("hex"))
print "a1 =", type(tester.a1), str(tester.a1.encode("hex")), type(tester.k1), str(tester.k1.encode("hex"))
print "a2 =", type(tester.a2), str(tester.a2.encode("hex")), type(tester.k2), str(tester.k2.encode("hex"))

os.system("mkdir -p good")
os.system("mkdir -p bad")
#os.system("./main predict --level 0 --prefix good")
#os.system("./main predict --level 0 --prefix bad --layer 2 --position 1234")

#hash_to_int = lambda x: int(x, 16)
hash_to_bytes32 = lambda x : x.decode('hex')

class Server:

	def __init__(self, a, k, prefix, predict_path, root_path):
                self.prefix = prefix;
		self.a = a;
		self.k = k;
		with open(predict_path, "r") as input_file:
			self.predict = int(input_file.readline())
		with open(root_path, "r") as input_file:
			self.root_hash = map(hash_to_bytes32, input_file.readline().split())
			self.tree_size = map(int, input_file.readline().split())

	def submitPrediction(self):
		contract.submitPrediction(self.predict, self.root_hash, self.tree_size, sender = self.k)

        def get_position(self, layer, position):
                if layer == 0:
                        filename = "Layer_data_input_-1_output.txt"
                else:
                        filename = FILES[layer].replace('hashtree', 'output')
                with open(os.path.join(self.prefix, filename), "r") as input_file:
                        data = map(int, input_file.read().split())
                return data[position]

	def loadHashTree(self, layer):
		filename = os.path.join(self.prefix, FILES[layer])
		with open(filename, "r") as input_file:
			self.hash_tree = map(hash_to_bytes32, input_file.readline().split())

	def submitHash(self, l, r):

		def get_id(l, r):
			return (l + r) | int(l != r)

		m = (l + r) >> 1
		l_id = get_id(l, m)
		r_id = get_id(m + 1, r)
#		print self.hash_tree[l_id], self.hash_tree[r_id]
		contract.submitHash(self.hash_tree[l_id], self.hash_tree[r_id], sender = self.k)

	def submitPathHash(self, layer, pos):
                reluInput = self.get_position(layer, pos)

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
#			from Crypto.Hash import SHA256
#			sha2 = lambda x : SHA256.new(x).digest()
#			print sha2(self.hash_tree[l_id] + self.hash_tree[r_id]) == sha2(self.hash_tree[get_id(l, r)])
			if pos <= m:
				r = m
			else:
				l = m + 1
#		for x in l_hash:
#			print x
#		for x in r_hash:
#			print x
		
		return contract.submitPathHash(reluInput, parent_hash, l_hash, r_hash, sender = self.k)

alice = Server(tester.a1, tester.k1, "good/predict.txt", "good/hashtree_roots.txt", "good")
bob = Server(tester.a2, tester.k2, "bad/predict.txt", "bad/hashtree_roots.txt", "bad")

alice.submitPrediction()
bob.submitPrediction()

if contract.verifyEquality(sender = tester.k0):
	print "Exactly correct!"
	sys.exit(0)

print "Somewhere incorrect!"

layer = contract.getAt(sender = tester.k0)

print "First mismatch layer is", layer

alice.loadHashTree(layer)
bob.loadHashTree(layer)

while True:
	l, r = contract.getBound(sender = tester.k0)
#	print "[l, r] = [%d, %d]" % (l, r)
	alice.submitHash(l, r)
	bob.submitHash(l, r)
	if contract.determineDirection(sender = tester.k0):
		break

position, _ = contract.getBound(sender = tester.k0)
print "different_position =", position

os.system(("./main interactive --level 1 --position %d --layer %d --prefix good") % (position, layer))

alice.loadHashTree(layer - 1)

if not alice.submitPathHash(layer - 1, position):
	print "Data corrupted"
	sys.exit(-1)

input_data = get_position("good", layer - 1, position)
alice_result = get_position("good", layer, position)
bob_result = get_position("bad", layer, position)

print input_data, alice_result, bob_result

who = contract.verifyReLU(input_data, alice_result, bob_result)

if who == 0:
	print "Alice is wrong"
else:
	print "Bob is wrong"

	
