open QueryTypes;

exception ParseError(string);

type queryObj = Belt.Map.String.t(queryValue);

let toQueryObj: Js.String.t => queryObj;

let string: (string, queryObj) => option(string);
let array: (string, queryObj) => option(array(string));
let default: ('a, option('a)) => 'a;
