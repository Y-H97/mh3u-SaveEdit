# Unit tests

Unit tests for the non-GUI core logic of the MH3U Save Editor. Before this
suite the project had **no tests** (0% coverage everywhere), so these target the
pure-logic modules that are most testable and highest value:

| Module | File | What is tested |
| --- | --- | --- |
| `MH3U_Armory` | `source/app/MH3U_Save_Editor/mh3u_se.cpp` | equipment ⇄ armor/charm/weapon conversion, little-endian packing, subtype mapping, round-trips |
| `MH3U_SE` | `source/app/MH3U_Save_Editor/mh3u_se.cpp` | save-file offset parsing, load → save → reload round-trip, in-memory edits, error paths |
| `MH3U_DS` | `source/app/MH3U_Save_Editor/mh3u_ds.cpp` | localized data-file parsing, language switching, accessors, cleanup |

The Qt widget modules (`source/app/widget/*`, `mh3u_sv.cpp`) are GUI code and are
not covered here.

## Running

```sh
cd tests
make test      # build and run the tests
make coverage  # run with gcov and print line coverage for the core modules
make clean
```

Only a C++11 compiler (`g++`) is required — no Qt and no third-party test
dependencies. The framework is the small header-only harness in
`test_framework.hpp` / `test_framework.cpp`.

Current core-module line coverage: `mh3u_se.cpp` ~94%, `mh3u_ds.cpp` ~81%.
