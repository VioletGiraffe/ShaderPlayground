# Your fragment shader playground

A tool for live-editing fragment shaders. Mostly compatible with [Shadertoy.com](https://www.shadertoy.com/).

![Screenshot](https://user-images.githubusercontent.com/1175926/165726676-b9f1455b-6cc0-4591-abfa-e91d49741999.png)

### Known Issues
For the list of known issues, refer to the project issues on Github, sort by the "bug" label. Or just use <a href="https://github.com/VioletGiraffe/ShaderPlayground/labels/bug">this link</a>.

### Reporting an issue
<a href="https://github.com/VioletGiraffe/ShaderPlayground/issues/new">Create an issue</a> on the project's page on Github.

### Contributing

Any contributions are welcome!

***Cloning the repository***

   The main git repository has submodules, so you need to execute the `update_repository` script (available as .bat for Windows and .sh for Win / Mac) after cloning this repo in order to clone the nested repositories. Subsequently, you can use the same `update_repository` script at any time to pull incoming changes to the main repo, as well as to all the subrepos, thus updating everything to the latest revision.

***Building***

* A compiler with C++ 14 support is required.
* Qt 5.8 or newer required.
* Windows: you can build using either Qt Creator or Visual Studio for IDE. Visual Studio 2015 or newer is required - v140 toolset or newer. Run `qmake -tp vc -r` to generate the solution for Visual Studio. I have not tried building with MinGW, but it should work as long as you enable C++ 14 support.
* Linux: open the project file in Qt Creator and build it.
* Mac OS X: You can use either Qt Creator (simply open the project in it) or Xcode (run `qmake -r -spec macx-xcode` and open the Xcode project that's been generated).
