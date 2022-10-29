const HtmlWebpackPlugin = require('html-webpack-plugin');
const AddAssetHtmlPlugin = require('add-asset-html-webpack-plugin');

var webpack = require('webpack');

module.exports = require('./scalajs.webpack.config');

module.exports.plugins = (module.exports.plugins || []).concat([
  new HtmlWebpackPlugin({
    title: 'Circle of the Moon Randomizer'
  }),
  new AddAssetHtmlPlugin([
	{filepath: require.resolve('./cotm_loadhook.js')},
	{filepath: require.resolve('./cotm_presets.js')},
	{filepath: require.resolve('./cotmr_core.js')}
  ])
]);
