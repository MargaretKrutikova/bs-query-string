type queryValue =
  | Single(string)
  | Multiple(array(string));

let stringify: array((string, queryValue)) => string;
let addToPath: (~path: string, ~qs: string) => string;

type queryObj = Belt.Map.String.t(queryValue);
let parse: Js.String.t => queryObj;
