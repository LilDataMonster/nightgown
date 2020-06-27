module.exports = {
  siteMetadata: {
    siteTitle: `Lil Data Monster - Jacket of Sleep`,
    defaultTitle: `Lil Data Monster - Jacket of Sleep`,
    siteTitleShort: `Jacket of Sleep`,
    siteDescription: `Lil Data Monster Jacket of Sleep device`,
    siteUrl: `https://rocketdocs.netlify.com`,
    siteAuthor: `David Fan`,
    siteImage: `/banner.png`,
    siteLanguage: `en`,
    themeColor: `#cf2ccb`,
    basePath: `/`,
    footer: `Lil Data Monster`,
  },
  plugins: [
    {
      resolve: `@rocketseat/gatsby-theme-docs`,
      options: {
        configPath: `src/config`,
        docsPath: `src/docs`,
        githubUrl: `https://github.com/rocketseat/gatsby-themes`,
        baseDir: `examples/gatsby-theme-docs`,
      },
    },
    {
      resolve: `gatsby-plugin-manifest`,
      options: {
        name: `Rocketseat Gatsby Themes`,
        short_name: `RS Gatsby Themes`,
        start_url: `/`,
        background_color: `#ffffff`,
        display: `standalone`,
        icon: `static/favicon.png`,
      },
    },
    `gatsby-plugin-sitemap`,
    {
      resolve: `gatsby-plugin-google-analytics`,
      options: {
        // trackingId: ``,
      },
    },
    {
      resolve: `gatsby-plugin-canonical-urls`,
      options: {
        siteUrl: `https://rocketdocs.netlify.com`,
      },
    },
    `gatsby-plugin-offline`,
  ],
};
