exception ParseError(string);

type queryValue =
  | Single(string)
  | Multiple(array(string));

type queryObj = Belt.Map.String.t(queryValue);

let encode: string => Js.String.t;

let parse: Js.String.t => queryObj;
let addToPath: (~path: string, ~qs: string) => string;

module Stringify: {
  let string: string => queryValue;
  let array: array(string) => queryValue;
  let make: array((string, queryValue)) => string;
};

module Parse: {
  let string: (string, queryObj) => option(string);
  let array: (string, queryObj) => option(array(string));
};
