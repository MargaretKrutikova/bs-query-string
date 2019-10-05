open Jest;
open Expect;

type testQueryObj = {
  query: option(string),
  array: option(array(string)),
};

describe("query string utils", () => {
  test("stringify works with single and multiple values", () => {
    let qs =
      Stringify.(
        toQs([|
          ("query", string("text")),
          ("filter", string("name")),
          ("array", array([|"a", "b", "c"|])),
        |])
      );

    // tested against https://www.urlencoder.io/
    expect(qs) |> toEqual("query=text&filter=name&array=a&array=b&array=c");
  });

  test("stringify allows to add individual query items to query array", () => {
    let queryArray =
      Stringify.(
        [|("query", string("text")), ("filter", string("name"))|]
      );

    let qs = Stringify.(toQs(queryArray |> add("forgot", string("smth"))));

    expect(qs) |> toEqual("query=text&filter=name&forgot=smth");
  });

  test("stringify allows to optionally add query item to query array", () => {
    let empty = "";

    let qa =
      Stringify.(
        [|("query", string("text")), ("filter", string("name"))|]
      );

    let qs =
      Stringify.(
        toQs(qa |> addOptional("empty", string(empty), ~skip=empty == ""))
      );

    expect(qs) |> toEqual("query=text&filter=name");
  });

  test("stringify encodes special characters correctly", () => {
    let qs =
      Stringify.(
        toQs([|
          ("query", string("search *1! -/0'")),
          ("array", array([|";_=.,+", "b~()[[]"|])),
        |])
      );

    expect(qs)
    |> toEqual(
         "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D",
       );
  });

  test("stringify doesn't include empty arrays", () => {
    let qs =
      Stringify.(
        toQs([|("query", string("text")), ("array", array([||]))|])
      );
    expect(qs) |> toEqual("query=text");
  });

  test("parse correctly decodes single and multiple values", () => {
    let qs = "query=text&filter=name&array=a&array=b&array=c";

    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        query: queryObj |> string("query"),
        array: queryObj |> array("array"),
      };

    expect(parsedObj)
    |> toEqual({query: Some("text"), array: Some([|"a", "b", "c"|])});
  });

  test("parse decodes special characters correctly", () => {
    let qs = "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        query: queryObj |> string("query"),
        array: queryObj |> array("array"),
      };

    expect(parsedObj)
    |> toEqual({
         query: Some("search *1! -/0'"),
         array: Some([|";_=.,+", "b~()[[]"|]),
       });
  });
});
