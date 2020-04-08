open QueryTypes;

exception ParseError(string);

type queryObj = Belt.Map.String.t(queryValue);
type parser('a) = string => 'a;

let addToMap = (map, keyValue) => {
  switch (Js.String.split("=", keyValue)) {
  | [|"", ""|] => map
  | [|key, encodedValue|] =>
    let valueToAdd = Js.Global.decodeURIComponent(encodedValue);
    let newValue =
      switch (map->Belt.Map.String.get(key)) {
      | None => Single(valueToAdd)
      | Some(Single(singleVal)) => Multiple([|singleVal, valueToAdd|])
      | Some(Multiple(multiVal)) =>
        Multiple(multiVal->Belt.Array.concat([|valueToAdd|]))
      };
    map->Belt.Map.String.set(key, newValue);
  | _ => map
  };
};

let toQueryObj = (query): queryObj => {
  let queryMap = Belt.Map.String.empty;

  let pairs = Js.String.split("&", query);
  Array.fold_left(addToMap, queryMap, pairs);
};

let item = (key, queryObj: queryObj) => {
  switch (queryObj->Belt.Map.String.get(key)) {
  | Some(value) => value
  | None =>
    raise(ParseError("Value with key: " ++ key ++ ", is not present"))
  };
};

let single = (key, parse: parser('a), queryObj) => {
  switch (queryObj |> item(key)) {
  | Single(val_) => parse(val_)
  | Multiple(array) =>
    let strArray = array |> Js.Json.stringArray |> Js.Json.stringify;
    raise(
      ParseError(
        "Expected single value, for key '" ++ key ++ "', got " ++ strArray,
      ),
    );
  };
};

let array = (key, parse: parser('a), queryObj) => {
  switch (queryObj |> item(key)) {
  | Single(val_) => [|parse(val_)|]
  | Multiple(array) => array->Belt.Array.map(parse)
  };
};

let string: parser(string) = queryValue => queryValue;

let parseSafe = (parse: parser('a), qs) =>
  try (Some(qs |> parse)) {
  | _ => None
  };

let optional = (parse: queryObj => 'a, queryObj: queryObj): option('a) =>
  try (Some(queryObj |> parse)) {
  | ParseError(_) => None
  };

let withDefault = (default, parse, queryObj) =>
  try (queryObj |> parse) {
  | _ => default
  };

let withEmptyArray = (key, parse, queryObj) =>
  withDefault([||], array(key, parse), queryObj);
