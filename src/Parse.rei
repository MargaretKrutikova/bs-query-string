open QueryTypes;

exception ParseError(string);

type queryObj = Belt.Map.String.t(queryValue);
type parser('a) = queryValue => 'a;

let toQueryObj: Js.String.t => queryObj;

let string: parser(string);
let array: parser(array(string));

let item: (string, parser('a), queryObj) => 'a;
let optional: (queryObj => 'a, queryObj) => option('a);
let withDefault: ('a, queryObj => 'a, queryObj) => 'a;
