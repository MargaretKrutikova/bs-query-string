open QueryTypes;

type queryItem = (string, queryValue);
type queryArray = array(queryItem);

let encode: string => Js.String.t;

let toQs: queryArray => string;

let string: string => queryValue;
let array: array(string) => queryValue;
let add: (string, queryValue, queryArray) => queryArray;
let addOptional: (string, queryValue, ~skip: bool, queryArray) => queryArray;
