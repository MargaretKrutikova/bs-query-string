type queryValue =
  | Single(string)
  | Multiple(array(string));
let encode: string => Js.String.t;

module Stringify: {
  let string: string => queryValue;
  let array: array(string) => queryValue;

  let make: array((string, queryValue)) => string;
  let addToPath: (~path: string, ~qs: string) => string;
};

module Parse: {
  exception ParseError(string);
  type queryObj = Belt.Map.String.t(queryValue);

  let parse: Js.String.t => queryObj;
  let string: (string, queryObj) => option(string);
  let array: (string, queryObj) => option(array(string));
};
