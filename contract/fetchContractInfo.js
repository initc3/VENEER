// var name = "Alice" or "Bob"
// var address = "0x{alice}" or "0x{bob}"
//    alice: 0xD6B08EAf2C60887B8BB0f013643B749f8C2fA1Ff
//    bob:   0xD2ABD421BE57057520c22EcC65c30FBbE77e3dE8
// customer: 0x3b49882D180317ADd06e92562C0e7A004c3d28C3

var contractAddress = '0xA877CFed5d7E89a7b2A767429Eb08c905ee2F5CB';

var abi = [{"constant":true,"inputs":[{"name":"","type":"address"}],"name":"servers","outputs":[{"name":"addr","type":"address"},{"name":"predict","type":"int256"},{"name":"lHash","type":"bytes32"},{"name":"rHash","type":"bytes32"}],"type":"function"},{"constant":true,"inputs":[],"name":"getAt","outputs":[{"name":"result","type":"int256"}],"type":"function"},{"constant":false,"inputs":[],"name":"testSender","outputs":[{"name":"a","type":"address"}],"type":"function"},{"constant":true,"inputs":[],"name":"getBound","outputs":[{"name":"l","type":"int256"},{"name":"r","type":"int256"}],"type":"function"},{"constant":false,"inputs":[{"name":"lHash","type":"bytes32"},{"name":"rHash","type":"bytes32"}],"name":"submitHash","outputs":[],"type":"function"},{"constant":true,"inputs":[],"name":"verifyReLU","outputs":[{"name":"who","type":"int256"}],"type":"function"},{"constant":false,"inputs":[],"name":"verifyEquality","outputs":[{"name":"isEqual","type":"bool"}],"type":"function"},{"constant":false,"inputs":[{"name":"predict","type":"int256"},{"name":"rootHash","type":"bytes32[14]"},{"name":"length","type":"int256[14]"}],"name":"submitPrediction","outputs":[],"type":"function"},{"constant":false,"inputs":[],"name":"determineDirection","outputs":[{"name":"reachLeaf","type":"bool"}],"type":"function"},{"constant":false,"inputs":[{"name":"input","type":"int256"},{"name":"output","type":"int256"},{"name":"parent_hash","type":"bytes32[]"},{"name":"l_hash","type":"bytes32[]"},{"name":"r_hash","type":"bytes32[]"}],"name":"submitPathHash","outputs":[{"name":"correct","type":"bool"}],"type":"function"},{"inputs":[{"name":"_alice","type":"address"},{"name":"_bob","type":"address"},{"name":"_input","type":"bytes32"}],"type":"constructor"}]


var contract = web3.eth.contract(abi);
var instance = contract.at(contractAddress);
