module.exports = {
  siteMetadata: {
    siteTitle: `Lil Data Monster - Nightgown`,
    defaultTitle: `Lil Data Monster - Nightgown`,
    siteTitleShort: `Jacket of Sleep`,
    siteDescription: `Lil Data Monster Nightgown`,
    siteUrl: `https://lildatamonster.github.io/nightgown/`,
    siteAuthor: `David Fan`,
    siteImage: `/banner.png`,
    siteLanguage: `en`,
    themeColor: `#cf2ccb`,
    basePath: `/nightgown/`,
    footer: `Lil Data Monster`,
  },
  plugins: [
    {
      resolve: `@rocketseat/gatsby-theme-docs`,
      options: {
        configPath: `src/config`,
        docsPath: `src/docs`,
        githubUrl: `https://github.com/LilDataMonster/nightgown`,
        baseDir: `docs`,
      },
    },
    {
      resolve: `gatsby-plugin-manifest`,
      options: {
        name: `Nightgown`,
        short_name: `Nightgown`,
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
        siteUrl: `https://lildatamonster.github.io/nightgown/`,
      },
    },
    `gatsby-plugin-offline`,
  ],
};
