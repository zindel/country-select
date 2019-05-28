module Result = Belt.Result;

module CountryList = {
  let url = "https://gist.githubusercontent.com/rusty-key/659db3f4566df459bd59c8a53dc9f71f/raw/4127f9550ef063121c564025f6d27dceeb279623/counties.json";
  let promise = ref(None);

  let fetch = () =>
    switch (promise^) {
    | Some(promise) => promise
    | None =>
      let fetch' = () =>
        Js.Promise.(
          Fetch.fetch(url)
          |> then_(Fetch.Response.json)
          |> then_(json => resolve(ReactSelect.decodeOptions(json)))
          |> then_(options => resolve(Result.Ok(options)))
          |> catch(err => resolve(Result.Error(err)))
        );
      let promise' = fetch'();
      promise := Some(promise');
      promise';
    };
};

module SvgDropDownArrow = {
  [@react.component]
  let make = () =>
    <i
      style={
        ReactDOMRe.Style.make(~display="inline-block", ~marginLeft="8px", ())
      }>
      <svg width="0.7em" height="0.7em" viewBox="0 0 255 255">
        <path d="M0 63.75l127.5 127.5L255 63.75z" />
      </svg>
    </i>;
};

module SvgSearch = {
  [@react.component]
  let make = () =>
    <div style={ReactDOMRe.Style.make(~paddingLeft="8px", ())}>
      <svg viewBox="0 0 250.313 250.313" width="1em" height="1em">
        <path
          d="M244.186 214.604l-54.379-54.378c-.289-.289-.628-.491-.93-.76 10.7-16.231 16.945-35.66 16.945-56.554C205.822 46.075 159.747 0 102.911 0S0 46.075 0 102.911c0 56.835 46.074 102.911 102.91 102.911 20.895 0 40.323-6.245 56.554-16.945.269.301.47.64.759.929l54.38 54.38c8.169 8.168 21.413 8.168 29.583 0 8.168-8.169 8.168-21.413 0-29.582zm-141.275-44.458c-37.134 0-67.236-30.102-67.236-67.235 0-37.134 30.103-67.236 67.236-67.236 37.132 0 67.235 30.103 67.235 67.236s-30.103 67.235-67.235 67.235z"
          fillRule="evenodd"
          clipRule="evenodd"
        />
      </svg>
    </div>;
};

module MenuList = {
  open ReactVirtualized;

  let isFocused: React.element => bool = [%raw
    "function(el) {return el.props.isFocused}"
  ];

  [@react.component]
  let make = (~children) => {
    let rows =
      Js.Array.isArray(children) ? Belt.Option.getExn(children) : [||];
    let rowRenderer: List.rowRenderer =
      row =>
        <div
          key=row##key
          style=ReactDOMRe.(
            Style.combine(row##style, Style.make(~whiteSpace="nowrap", ()))
          )>
          {Js.Array.unsafe_get(rows, row##index)}
        </div>;

    /* this is needed to support keyboard navigation */
    let scrollToIndex = Js.Array.findIndex(isFocused, rows);

    <List
      style={ReactDOMRe.Style.make(~width="100%", ())}
      width=300
      height=300
      rowHeight=30
      rowCount={Js.Array.length(rows)}
      rowRenderer
      scrollToIndex={
        scrollToIndex == (-1) ?
          Js.Undefined.empty : Js.Undefined.return(scrollToIndex)
      }
    />;
  };
};

type state = {
  isOpen: bool,
  keepFocus: bool,
  options,
}
and options =
  | Loading
  | Error
  | Options(ReactSelect.options);

type action =
  | Toggle(bool)
  | SetCountryList(Result.t(ReactSelect.options, Js.Promise.error))
  | ReleaseFocus;

let reducer = (state, action) =>
  switch (action) {
  | Toggle(keepFocus) => {...state, isOpen: !state.isOpen, keepFocus}
  | SetCountryList(Result.Error(err)) =>
    Js.log(err);
    {...state, options: Error};
  | SetCountryList(Result.Ok(options)) => {
      ...state,
      options: Options(options),
    }
  | ReleaseFocus => {...state, keepFocus: false}
  };

let focusedColor = "blue";
let normalColor = "hsla(218, 50%, 10%, 0.1)";

[@bs.send] external focus: Dom.element => unit = "focus";

let styles =
  ReactSelect.styles(
    ~control=
      _ =>
        ReactDOMRe.Style.make(
          ~display="flex",
          ~alignItems="center",
          ~flexDirection="row-reverse",
          ~minHeight="38",
          ~outline="0",
          ~minWidth="240",
          (),
        ),
    ~menu=
      _ =>
        ReactDOMRe.Style.make(
          ~boxShadow="inset 0 1px 0 rgba(0, 0, 0, 0.1)",
          (),
        ),
    (),
  );

let components =
  ReactSelect.components(
    ~menuList=Js.Null.return(MenuList.make),
    ~indicatorSeparator=Js.Null.empty,
    ~dropdownIndicator=Js.Null.return(SvgSearch.make),
    (),
  );

let formatOptionLabel = (item, _) =>
  <div>
    <span className={"flag-icon flag-icon-" ++ ReactSelect.valueGet(item)} />
    <span> {React.string(ReactSelect.labelGet(item))} </span>
  </div>;

[@react.component]
let make = (~className=?, ~country, ~onChange) => {
  let onChange = item => {
    let selected = ReactSelect.valueGet(item);
    switch (country) {
    | Some(country) when country == selected => ()
    | _ => onChange(selected)
    };
  };

  let ({isOpen, keepFocus, options}, dispatch) =
    React.useReducer(
      reducer,
      {isOpen: false, keepFocus: false, options: Loading},
    );

  let buttonRef = React.useRef(Js.Nullable.null);
  React.useEffect(() => {
    if (keepFocus) {
      Js.Nullable.iter(React.Ref.current(buttonRef), (. el) => focus(el));
      dispatch(ReleaseFocus);
    };
    None;
  });

  React.useEffect1(
    () => {
      Js.Promise.(
        switch (options) {
        | Loading =>
          CountryList.fetch()
          |> then_(result => resolve(dispatch(SetCountryList(result))))

        | _ => resolve()
        }
      )
      |> ignore;
      None;
    },
    [|options|],
  );

  let getSelectedIndex =
    React.useCallback2(
      options =>
        switch (country) {
        | None => None
        | Some(value) =>
          let index =
            Js.Array.findIndex(
              item => ReactSelect.valueGet(item) == value,
              options,
            );
          switch (index) {
          | (-1) => None
          | index => Some(index)
          };
        },
      (options, country),
    );

  let getSelectedOption = options =>
    switch (getSelectedIndex(options)) {
    | None => None
    | Some(index) => Some(Js.Array.unsafe_get(options, index))
    };

  let onKeyDown = e =>
    switch (options) {
    | Options([||])
    | Options([|_|])
    | Error
    | Loading => ()
    | Options(options) =>
      let getNextOption = () => {
        let index =
          Belt.Option.getWithDefault(getSelectedIndex(options), -1) + 1;
        let index = index == Js.Array.length(options) ? 0 : index;
        Js.Array.unsafe_get(options, index);
      };

      let getPrevOption = () => {
        let index =
          Belt.Option.getWithDefault(getSelectedIndex(options), -1) - 1;
        let index = index < 0 ? Js.Array.length(options) - 1 : index;
        Js.Array.unsafe_get(options, index);
      };
      switch (isOpen, ReactEvent.Keyboard.key(e)) {
      | (false, "ArrowUp") => onChange(getPrevOption())
      | (false, "ArrowDown") => onChange(getNextOption())
      | _ => ()
      };
    };

  <div
    className={
      "country-select"
      ++ Belt.Option.mapWithDefault(className, "", cls => " " ++ cls)
    }>
    <button
      ref={ReactDOMRe.Ref.domRef(buttonRef)}
      style={
        ReactDOMRe.Style.make(
          ~borderColor=isOpen ? focusedColor : normalColor,
          (),
        )
      }
      onClick={_ => dispatch(Toggle(false))}
      onKeyDown>
      <span>
        {
          React.string(
            switch (options) {
            | Loading => "Loading..."
            | Error => "Error! Check console"
            | Options(options) =>
              Belt.Option.mapWithDefault(
                getSelectedOption(options),
                "",
                ReactSelect.labelGet,
              )
            },
          )
        }
      </span>
      <SvgDropDownArrow />
    </button>
    {
      switch (isOpen, options) {
      | (true, Options(options)) =>
        let value = Js.Nullable.fromOption(getSelectedOption(options));
        let onChange = item => {
          dispatch(Toggle(true));
          onChange(item);
        };
        let onKeyDown = e =>
          switch (ReactEvent.Keyboard.key(e)) {
          | "Escape" => dispatch(Toggle(true))
          | "Tab" => dispatch(Toggle(false))
          | _ => ()
          };
        <React.Fragment>
          <div className="country-select-menu">
            <ReactSelect
              autoFocus=true
              backspaceRemovesValue=false
              components
              controlShouldRenderValue=false
              hideSelectedOptions=false
              formatOptionLabel
              isClearable=false
              menuIsOpen=true
              onChange
              options
              placeholder="Search..."
              styles
              tabSelectsValue=false
              value
              onKeyDown
            />
          </div>
          <div
            className="country-select-clickaway"
            onClick=(_ => dispatch(Toggle(true)))
          />
        </React.Fragment>;
      | _ => React.null
      }
    }
  </div>;
};
