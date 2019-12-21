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

The first value is the error level of the rule and can be one of these values:

- "off" or 0 - turn the rule off
- "warn" or 1 - turn the rule on as a warning (doesn't affect exit code)
- "error" or 2 - turn the rule on as an error (exit code will be 1)

The three error levels allow you fine-grained control over how ESLint applies rules.

- code fix

```
npx eslint --fix src/js/mcg.js 
```
