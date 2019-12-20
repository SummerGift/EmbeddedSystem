### Getting Started with ESLint

- install ESLint using npm or yarn:
```
npm install eslint --save-dev

# or

yarn add eslint --dev
```

- set up a configuration file
```
npx eslint --init
```

- run ESLint on any file or directory
```
npx eslint yourfile.js
```

### ESLint Formatters

- format sourcecode

```
npx eslint --format codeframe yourfile.js
```

- configuration

```
.eslintrc:
{
    "extends": "eslint:recommended",
    "rules": {
        "consistent-return": 2,
        "indent"           : [1, 4],
        "no-else-return"   : 1,
        "semi"             : [1, "always"],
        "space-unary-ops"  : 2
    }
}
```

- code fix

```
npx eslint --fix src/js/mcg.js 
```