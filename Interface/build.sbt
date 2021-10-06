enablePlugins(ScalaJSPlugin)

name := "CotmInterface"

version := "0.1"

scalaVersion := "2.13.5"

scalaJSUseMainModuleInitializer := true

libraryDependencies += "com.github.japgolly.scalajs-react" %%% "core" % "1.7.7"

libraryDependencies += "com.github.japgolly.scalajs-react" %%% "ext-monocle-cats" % "1.7.7"
libraryDependencies += "com.github.julien-truffaut" %%% "monocle-core" % "2.0.4"
libraryDependencies += "com.github.julien-truffaut" %%% "monocle-macro" % "2.0.4"

libraryDependencies += "org.scala-js" %%% "scalajs-dom" % "1.0.0"

npmDependencies in Compile ++= Seq(
  "react" -> "16.7.0",
  "react-dom" -> "16.7.0",

  "html-webpack-plugin" -> "3.2.0",
  "add-asset-html-webpack-plugin" -> "3.2.0"
)

webpackConfigFile := Some(baseDirectory.value / "cotminterface.webpack.config.js")

enablePlugins(ScalaJSBundlerPlugin)

scalacOptions in Global += "-Ymacro-annotations"
