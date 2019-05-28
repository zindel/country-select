# CountrySelect in Reason

Assuming you have the latest [esy](https://esy.sh/) globally installed.

# Development
```Bash
$ yarn
$ esy install
$ yarn start
$ yarn server # in separate terminal
```

Application could be accessed at `http://localhost:3002`

# Building

```Bash
$ yarn
$ esy install
$ yarn build
$ ls -l ./build
```

# Notes
- `esy` may not work with `fnm` seamlessly, in this case remove the `_esy`
  directory and try again steps above
- the steps above were tested on MacOS only (Linux should work fine).
