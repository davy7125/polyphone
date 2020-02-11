Use these instructions if you cannot install Polyphone on your Mac OS X with the available installers in the section “[Download][download]”.


## Prerequisite


The following applications are required:

* Xcode (available in the Apple Store),
* [Qt Creator with its framework][get-qt].

You also need the sources of Polyphone, available [here][download] or on [GitHub][polyphone], along with the necessary libraries you can download [here][lib_mac].


## Build


Unzip the libraries and place the directory :file:`lib_mac` just next to the directory :file:`sources`.

Open the file :file:`polyphone.pro` with :program:`Qt Creator`.
Build the project, a bundle :file:`polyphone.app` should appear in the directory :file:`lib_mac`.

If the SDK path cannot be resolved, try modifying the file :file:`polyphone.pro` to include these variables (adjust the Mac OSX version of first):

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
QMAKE_MAC_SDK = macosx10.11
```

## Complete the bundle


Run the following command to include libraries and frameworks inside the bundle (adjust the path first!):

```
/Path/To/Qt/5.2.0/clang_64/bin/macdeployqt /Path/To/lib_mac/polyphone.app
```

Copy the framework Jackmp from :file:`lib_mac` and paste it to :file:`lib_mac/polyphone.app/Contents/Frameworks`.
You may need to right click and select :guilabel:`View content` to go inside the bundle instead of executing it.

Finally, run the following command in a single line (adjust the last path first!):

```
install_name_tool -change /System/Library/Frameworks/Jackmp.framework/Versions/A/Jackmp @executable_path/../Frameworks/Jackmp.framework/Versions/A/Jackmp /Path/To/lib_mac/polyphone.app/Contents/MacOS/Polyphone
```

You can then execute the program or compress it as a .zip file to share it.


## Troubleshooting


See this [topic] if you need more information or for getting some help.


[download]:  download                               {target=_blank}
[get-qt]:    https://www.qt.io/download-open-source {target=_blank rel=noopener}
[polyphone]: https://github.com/davy7125/polyphone  {target=_blank rel=noopener}

[lib_mac]: downloads/lib_mac.zip
[topic]:   forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7
