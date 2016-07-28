contract VENEER {

    struct Server {
        address addr;         // the address of the server
        int predict;          // the prediction made by the server
        HashTree[14] tree;    // the hash code of root of each tree

        bytes32 lHash;           // hash of left child
        bytes32 rHash;           // hash of right child
    }

    struct HashTree {
        int size;
        bytes32 rootHash;
    }

    modifier onlyCustomer() {
        if (msg.sender != customer)
            throw;
        _
    }

    modifier onlyServer() {
        if (servers[msg.sender].addr != msg.sender)
            throw;
        _
    }

    modifier onlyAlice() {
        if (servers[alice].addr != msg.sender)
            throw;
        _
    }

    modifier customerOrServer() {
        if (msg.sender != customer && servers[msg.sender].addr != msg.sender)
            throw;
        _
    }

    address customer;
    address alice;
    address bob;
    mapping (address => Server) public servers;

    int at;               // we are going to verify the tree 'at'
    int lBound;           // which interval the node represents [lBound, rBound]
    int rBound;
    bytes32 input;

	bool dataCorrupted;
    int reluInput;
	int aliceOutput;

    function VENEER(address _alice, address _bob, bytes32 _input) {
        customer = msg.sender;
        alice = _alice;
        bob = _bob;
        input = _input;

        if (msg.sender == alice || msg.sender == bob || alice == bob)
            throw;

        at = -1;
        lBound = rBound = -1;

        Server a = servers[alice];
        a.addr = alice;
        a.predict = -1;
        a.lHash = 0;
        a.rHash = 0;

        Server b = servers[bob];
        b.addr = bob;
        b.predict = -1;
        b.lHash = 0;
        b.rHash = 0;

		dataCorrupted = false;
		reluInput = -1;
		aliceOutput = -1;
    }

	function testSender() returns (address a) {
		a = msg.sender;
		return a;
	}

    function submitPrediction(int predict, bytes32[14] rootHash, int[14] length) onlyServer {
        Server server = servers[msg.sender];
        if (server.predict != -1 || at != -1)
            throw;
        server.predict = predict;
        for (uint i = 0; i < 14; ++i)
            server.tree[i] = HashTree({size : length[i], rootHash : rootHash[i]});
	}

    function verifyEquality() onlyCustomer returns (bool isEqual) {
        for (uint i = 0; i < 14; ++i)
            if (servers[alice].tree[i].rootHash != servers[bob].tree[i].rootHash) {
                at = int(i);
                lBound = 0;
                rBound = servers[alice].tree[i].size - 1;
                return false;
            }
    }
    
    function getAt() constant returns (int result) {
        result = at;
    }

    function getBound() constant returns (int l, int r) {
        l = lBound;
        r = rBound;
    }
    
    function submitHash(bytes32 lHash, bytes32 rHash) onlyServer {
        Server server = servers[msg.sender];
        server.lHash = lHash;
        server.rHash = rHash;
    }

    function determineDirection() onlyCustomer returns (bool reachLeaf) {
        if (servers[alice].lHash != servers[bob].lHash) {
            rBound = (lBound + rBound) / 2;
        }
        else if (servers[alice].rHash != servers[bob].rHash) {
            lBound = (lBound + rBound) / 2 + 1;
        }
        else {
            throw;
        }
        reachLeaf = (lBound == rBound);
    }

    function verifyReLU() constant onlyCustomer returns (int who) {
		if (dataCorrupted) {
			who = 0;
			return;
		}
		int input = reluInput;
    	if (input < 0)
    		input = 0;
    	if (aliceOutput != input)
    		who = 0;
    	else
    		who = 1;
    }

	function submitPathHash(int input, int output, bytes32[] parent_hash, bytes32[] l_hash, bytes32[] r_hash) onlyAlice returns (bool correct) {
        Server server = servers[msg.sender];
		if (server.tree[uint(at - 1)].rootHash != parent_hash[0]) {
			correct = false;
			dataCorrupted = true;
			return;
		}
		uint i;
		for (i = 0; i < parent_hash.length; ++i) {
			if (sha256(l_hash[i], r_hash[i]) != parent_hash[i]) {
				correct = false;
				dataCorrupted = true;
				return;
			}
		}
		for (i = 1; i < parent_hash.length; ++i) {
			if (parent_hash[i] != l_hash[i - 1] && parent_hash[i] != r_hash[i - 1]) {
				correct = false;
				dataCorrupted = true;
				return;
			}
		}
		correct = true;
		reluInput = input;
		aliceOutput = output;
	}
}
