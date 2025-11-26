# Bug Report: VR Plugin Loading Failure

## Commit Hash
d911c58d9596b0ea085038f48244c7bfbf76c010

## Bug Report
When executing the VR oneshot test, the following error occurred:
```
FAILURE: VR audio failed to initialize: Error: Failed to load VR plugin: File not found.
```

The `resonanceaudio.dll` file was placed in the `bin` directory, so it was expected to be loadable.

## Root Cause
The test code in `examples/main.cpp` was attempting to load the Resonance Audio plugin using the path `lib\\resonanceaudio.dll`. However, since the executable runs from the `bin` directory and `resonanceaudio.dll` is located in the same `bin` directory, the relative path was incorrect.

The code was looking for the plugin at `bin\lib\resonanceaudio.dll` (relative to the executable's location), but the actual location was `bin\resonanceaudio.dll`.

## Fix Details
Updated all references in `examples/main.cpp` from `lib\\resonanceaudio.dll` to `resonanceaudio.dll`:

1. `testVrInitialization()` function (line 362, 376)
2. `testPluginInspector()` function (line 401)
3. `test3DOneshotSound()` function (line 454)

The plugin is now loaded using the correct relative path, allowing the VR audio initialization to succeed.
