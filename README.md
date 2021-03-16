# yaml
Change private-pub hosted dependencies in pubspec.yaml to pathed dependencies and vice versa.

````diff
   your_pkg_name:
-    version: 0.0.7
-    hosted:
-      name: your_pkg_name
-      url: *your_private_pub_url
+    path: path/to/your/package/your_pkg_name
````
