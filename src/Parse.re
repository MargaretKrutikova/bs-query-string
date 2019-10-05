open QueryTypes;

exception ParseError(string);

type queryObj = Belt.Map.String.t(queryValue);
type parser('a) = queryValue => 'a;

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

let item = (key, parse, queryObj) => {
  switch (queryObj->Belt.Map.String.get(key)) {
  | Some(value) => parse(value)
  | None =>
    raise(ParseError("Value with key: " ++ key ++ ", is not present"))
  };
};

let string: parser(string) =
  queryValue => {
    switch (queryValue) {
    | Single(val_) => val_
    | Multiple(array) =>
      let strArray = array |> Js.Json.stringArray |> Js.Json.stringify;
      raise(ParseError("Expected single value, got " ++ strArray));
    };
  };

let array: parser(array(string)) =
  queryValue => {
    switch (queryValue) {
    | Single(val_) => [|val_|]
    | Multiple(array) => array
    };
  };

let optional = (parse, queryObj) =>
  try (Some(queryObj |> parse)) {
  | ParseError(_) => None
  };

let withDefault = (default, parse, queryObj) =>
  try (queryObj |> parse) {
  | _ => default
  };
