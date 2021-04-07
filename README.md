# NP Plugin

###  https://github.com/axgle/npapi-chrome-plugin-helloworld

### register the plug in for Firefox52 on Windows

```
REG ADD HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\MozillaPlugins\npplugin.sample.mine
REG ADD HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\MozillaPlugins\npplugin.sample.mine /v Path /t REG_SZ /d xxx.dll
REG ADD HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\MozillaPlugins\npplugin.sample.mine\MimeTypes
REG ADD HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\MozillaPlugins\npplugin.sample.mine\MimeTypes\application/x-helloworld
```
