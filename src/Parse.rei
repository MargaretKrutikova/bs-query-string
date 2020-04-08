open QueryTypes;

exception ParseError(string);

type queryObj = Belt.Map.String.t(queryValue);
type parser('a) = string => 'a;

let toQueryObj: string => queryObj;

let single: (string, parser('a), queryObj) => 'a;
let array: (string, parser('a), queryObj) => array('a);

let string: parser(string);
let optional: (queryObj => 'a, queryObj) => option('a);

let withDefault: ('a, queryObj => 'a, queryObj) => 'a;
let withEmptyArray: (string, parser('a), queryObj) => array('a);
