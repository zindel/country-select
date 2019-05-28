[@bs.deriving abstract]
type item = {
  label: string,
  value: string,
};

[@bs.deriving abstract]
type components = {
  [@bs.optional] [@bs.as "MenuList"]
  menuList:
    Js.Null.t(
      {. "children": option(array(React.element))} => React.element,
    ),
  [@bs.optional] [@bs.as "DropdownIndicator"]
  dropdownIndicator: Js.Null.t(Js.t({.}) => React.element),
  [@bs.optional] [@bs.as "IndicatorSeparator"]
  indicatorSeparator: Js.Null.t(Js.t({.}) => React.element),
};

[@bs.deriving abstract]
type styles = {
  [@bs.optional]
  control: ReactDOMRe.Style.t => ReactDOMRe.Style.t,
  [@bs.optional]
  menu: ReactDOMRe.Style.t => ReactDOMRe.Style.t,
};

type options = Js.Array.t(item);

let decodeOptions =
  Json.Decode.(
    array(json =>
      item(
        ~label=field("label", string, json),
        ~value=field("value", string, json),
      )
    )
  );

type labelContext = {
  .
  "context": string,
  "inputValue": string,
  "selectValue": array(item),
};

[@bs.module "react-select"] [@react.component]
external make:
  (
    ~autoFocus: bool=?,
    ~backspaceRemovesValue: bool=?,
    ~components: components=?,
    ~controlShouldRenderValue: bool=?,
    ~formatOptionLabel: (item, labelContext) => React.element=?,
    ~hideSelectedOptions: bool=?,
    ~isClearable: bool=?,
    ~menuIsOpen: bool=?,
    ~onChange: item => unit=?,
    ~options: options=?,
    ~placeholder: string=?,
    ~styles: styles=?,
    ~tabSelectsValue: bool=?,
    ~value: Js.Nullable.t(item)=?,
    ~onKeyDown: ReactEvent.Keyboard.t => unit=?
  ) =>
  React.element =
  "default";
