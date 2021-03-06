# payload-extractor-plugin
This NSIS plugin extracts payload data from an installer who had this data embedded inside of it after it was digitally signed, unzips the resulting archive and places the files and folders in the specified destination path. The plugin can be used only with signed Windows installers that have had additional data injected, like the ones generated by <a href="https://github.com/sivu22/payload-injector">payload-injector</a>.<br>
<br>
The plugin consists of a single dynamically linked library PayExt.dll.

# Usage
Inside your NSIS script, assuming you want to extract data/configuration from the installer and copy it to the installation folder, you could write something like:<br>
<code>PayExt::ExtractPayload "$INSTDIR"</code> inside of the Post-install section.

# Implementation details
- Please keep in mind this is an ANSI project (due to NSIS constraint), if you are using by chance the Unicode NSIS and want to take advantage of the extended set, you need to change the project's character set to Unicode and rebuild.
- The payload data is limited to 1MB.
- Payload data must consist of a single valid ZIP archive. It's much easier this way to handle multiple files and folders.
- The source files, with minimum changes, can also be compiled with mingw32 if for example the installer package will be built under Linux.
