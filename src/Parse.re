open QueryTypes;

exception ParseError(string);

type queryObj = Belt.Map.String.t(queryValue);

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

let string = (key, queryObj) => {
  queryObj
  ->Belt.Map.String.get(key)
  ->Belt.Option.map(item =>
      switch (item) {
      | Single(val_) => val_
      | Multiple(array) =>
        let strArray = array |> Js.Json.stringArray |> Js.Json.stringify;
        raise(ParseError("Expected single value, got " ++ strArray));
      }
    );
};

let default = (default: 'a, value: option('a)) =>
  value->Belt.Option.getWithDefault(default);

let array = (key, queryObj) => {
  queryObj
  ->Belt.Map.String.get(key)
  ->Belt.Option.map(item =>
      switch (item) {
      | Single(val_) => [|val_|]
      | Multiple(array) => array
      }
    );
};
