// var name = "Alice" or "Bob"
// var address = "0x{alice}" or "0x{bob}"
//    alice: 0xD6B08EAf2C60887B8BB0f013643B749f8C2fA1Ff
//    bob:   0xD2ABD421BE57057520c22EcC65c30FBbE77e3dE8

// geth --exec 'var name="Alice"; var address="0xD6B08EAf2C60887B8BB0f013643B749f8C2fA1Ff"; loadScript("solidity/fetchContractInfo.js")' --testnet attach ipc:///home/amiller/.ethereum/testnet/geth.ipc

// geth --exec 'var name="Bob"; var address="0xD2ABD421BE57057520c22EcC65c30FBbE77e3dE8"; loadScript("solidity/fetchContractInfo.js")' --testnet attach ipc:///home/amiller/.ethereum/testnet/geth.ipc

var contractAddress = '0x4550fB11Cb65822465c5A23f31D66F9Db16D7D91';

var abi = [
    {"constant":true,
     "inputs":[{"name":"","type":"address"}],
     "name":"servers",
     "outputs":[
	 {"name":"addr","type":"address"},
	 {"name":"predict","type":"int256"},
	 {"name":"lHash","type":"uint256"},
	 {"name":"rHash","type":"uint256"}],
     "type":"function"},
    {"constant":false,"inputs":[{"name":"parent_hash","type":"bytes32[]"},{"name":"l_hash","type":"bytes32[]"},{"name":"r_hash","type":"bytes32[]"}],"name":"submitPathHash","outputs":[{"name":"correct","type":"bool"}],"type":"function"},
    {"constant":true,"inputs":[],"name":"getAt","outputs":[{"name":"result","type":"int256"}],"type":"function"},
    {"constant":false,"inputs":[],"name":"testSender","outputs":[{"name":"a","type":"address"}],"type":"function"},
    {"constant":false,"inputs":[{"name":"lHash","type":"uint256"},{"name":"rHash","type":"uint256"}],"name":"submitHash","outputs":[],"type":"function"},
    {"constant":true,"inputs":[],"name":"getBound","outputs":[{"name":"l","type":"int256"},{"name":"r","type":"int256"}],"type":"function"},
    {"constant":false,"inputs":[{"name":"input","type":"int256"},{"name":"aliceResult","type":"int256"},{"name":"bobResult","type":"int256"}],"name":"verifyReLU","outputs":[{"name":"who","type":"int256"}],"type":"function"},
    {"constant":false,"inputs":[],"name":"verifyEquality","outputs":[{"name":"isEqual","type":"bool"}],"type":"function"},
    {"constant":false,"inputs":[],"name":"determineDirection","outputs":[{"name":"reachLeaf","type":"bool"}],"type":"function"},
    {"constant":false,"inputs":[{"name":"predict","type":"int256"},{"name":"rootHash","type":"uint256[14]"},{"name":"length","type":"int256[14]"}],
     "name":"submitPrediction",
     "outputs":[],"type":"function"},{"inputs":[{"name":"_alice","type":"address"},{"name":"_bob","type":"address"},{"name":"_input","type":"bytes32"}],"type":"constructor"},
    {"constant":true,"inputs":[{"name":"input","type":"int256"},{"name":"aliceResult","type":"int256"},{"name":"bobResult","type":"int256"}],"name":"verifyReLU","outputs":[{"name":"who","type":"int256"}],"type":"function"}
]

var contract = web3.eth.contract(abi);
var instance = contract.at(contractAddress);

//function () {
//   instance.servers(address);
//}
