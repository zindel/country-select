/* [@bs.module "react-virtualized"] */
/* external virtualizedList: ReasonReact.reactClass = "List"; */

module List = {
  type row = {
    .
    "key": string,
    "index": int,
    "style": ReactDOMRe.Style.t,
  };

  type rowRenderer = row => React.element;

  [@bs.module "react-virtualized"] [@react.component]
  external make:
    (
      ~style: ReactDOMRe.Style.t=?,
      ~width: int=?,
      ~height: int=?,
      ~rowHeight: int=?,
      ~rowCount: int=?,
      ~rowRenderer: rowRenderer=?,
      ~scrollToIndex: Js.Undefined.t(int)=?
    ) =>
    React.element =
    "List";
};
