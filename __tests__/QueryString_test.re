open Jest;
open Expect;

describe("stringify", () => {
  test("works with single and multiple values", () => {
    let qs =
      QueryString.(
        stringify([|
          ("query", Single("text")),
          ("filter", Single("name")),
          ("array", Multiple([|"a", "b", "c"|])),
        |])
      );

    expect(qs) |> toEqual("query=text&filter=name&array=a&array=b&array=c");
  });

  test("encodes special characters", () => {
    let qs =
      QueryString.(
        stringify([|
          ("query", Single("search *1! -/0")),
          ("array", Multiple([|"; = ,+", "b", "c"|])),
        |])
      );

    expect(qs)
    |> toEqual(
         "query=search%20*1!%20-%2F0&array=%3B%20%3D%20%2C%2B&array=b&array=c",
       );
  });
});
