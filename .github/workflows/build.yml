name: Build Geode Mod

on:
  push:
  pull_request:
  workflow_dispatch:

jobs:
  build:
    name: Build (${{ matrix.target }})
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        target: [Windows, Android32, Android64]

    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Build Mod
        uses: geode-sdk/build-geode-mod@main
        with:
          target: ${{ matrix.target }}
