open Jest;
open Expect;

type paramsObj = {
  query: string,
  array: array(string),
};

type paramsObjWithOption = {
  optionQuery: option(string),
  array: array(string),
};

module CustomQueryParser = {
  type item = {
    id: string,
    index: int,
  };

  type urlQuery = {items: array(item)};

  let parseItem = (qs: string): item => {
    let obj = Parse.toQueryObj(qs);
    Parse.{
      id: single("id", string, obj),
      index: single("index", int_of_string, obj),
    };
  };

  let parseQuery = (qs: string): urlQuery => {
    let obj = Parse.toQueryObj(qs);
    let parsedItems =
      Parse.(
        obj |> withDefault([||], array("items", parseItem |> parseSafe))
      );

    {items: parsedItems->Belt.Array.keepMap(value => value)};
  };
};

describe("parse", () => {
  test("parses string and array values correctly", () => {
    let qs = "query=text&&array=a&array=b&array=c";

    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        query: queryObj |> single("query", string),
        array: queryObj |> array("array", string),
      };

    expect(parsedObj) |> toEqual({query: "text", array: [|"a", "b", "c"|]});
  });

  test("decodes special characters correctly", () => {
    let qs = "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        query: queryObj |> single("query", string),
        array: queryObj |> array("array", string),
      };

    expect(parsedObj)
    |> toEqual({query: "search *1! -/0'", array: [|";_=.,+", "b~()[[]"|]});
  });

  test("throws if value is not present in query string", () => {
    let qs = "array=1&array=2";
    let queryObj = Parse.toQueryObj(qs);
    let parse = () =>
      Parse.{
        query: queryObj |> single("query", string),
        array: queryObj |> array("array", string),
      };

    expect(parse) |> toThrow;
  });

  test("throws if array was parsed for single value", () => {
    let qs = "q=1&q=2&array=1&array=2";
    let queryObj = Parse.toQueryObj(qs);
    let parse = () =>
      Parse.{
        query: queryObj |> single("query", string),
        array: queryObj |> array("array", string),
      };

    expect(parse) |> toThrow;
  });

  test("parses optional values correctly", () => {
    let qs = "array=1&array=2";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        optionQuery: queryObj |> optional(single("query", string)),
        array: queryObj |> array("array", string),
      };

    expect(parsedObj) |> toEqual({optionQuery: None, array: [|"1", "2"|]});
  });

  test("parses default values correctly", () => {
    let qs = "";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        optionQuery: queryObj |> optional(single("query", string)),
        array:
          queryObj |> withDefault([|"1", "2"|], array("array", string)),
      };

    expect(parsedObj) |> toEqual({optionQuery: None, array: [|"1", "2"|]});
  });

  test("parses custom type correctly", () => {
    let qs = "items=id%3D123%26index%3D45&items=id%3D124%26index%3D46"; // id=123&index=45, id=124&index=46

    let parsedObj = CustomQueryParser.parseQuery(qs);

    let expected: array(CustomQueryParser.item) = [|
      {id: "123", index: 45},
      {id: "124", index: 46},
    |];

    expect(parsedObj.items) |> toEqual(expected);
  });

  test("doesn't throw exception when parsing custom type", () => {
    let qs = "items=id%3D123%26index%3D45&items=id%3D124%26index%3Dabc"; // id=123&index=45, id=124&index=abc

    let parsedObj = CustomQueryParser.parseQuery(qs);

    let expected: array(CustomQueryParser.item) = [|
      {id: "123", index: 45},
    |];

    expect(parsedObj.items) |> toEqual(expected);
  });
});
