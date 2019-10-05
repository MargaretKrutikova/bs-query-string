open QueryTypes;

type itemValue;
type queryItem = (string, itemValue);
type queryArray = array(queryItem);
type encoder('a) = 'a => queryValue;

let encode: string => Js.String.t;

let toQs: queryArray => string;

let string: string => queryValue;
let array: array(string) => queryValue;

let item: ('a, encoder('a)) => itemValue;
let omit: ('a, encoder('a), 'a => bool) => itemValue;
