Current revision:

https://github.com/Unity-Technologies/cecil, branch = unity-master, commit = 0d2cf2e1cf4ac9910df6fa40e7cc0829b3d3d4de

To update the build on Windows, perform the following steps
1) Build il2cpp.sln
2) From the il2cpp solution directory run :

	>dotnet build\tools\net6.0\Unity.IL2CPP.NetCoreBuilder.dll --build-cecil --cecil-source-dir=<path to your cecil checkout>

3) Manually update the commit revision above to be the revision of your checkout.  (The updater tool will attempt to perform this step automatically)
