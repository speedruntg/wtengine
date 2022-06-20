#!/usr/bin/env node
/**
 * @author Matthew Evans
 * @module wtfsystems/wtengine
 * @see README.md
 * @copyright MIT see LICENSE.md
 */

import wtf from './_common.cjs'
import 'inquirer'

const build = {
    engine: () => {
        wtf.config.gitURLs.forEach(gitURL => {
            wtf.runCommand(`git clone ${gitURL.url}`, { cwd: wtf.constants.WORK_FOLDER })
        })
    },
    project: () => {}
}

process.stdout.write(`${wtf.colors.CYAN}WTEngine Build Utility${wtf.colors.CLEAR}\n\n`)

const args = wtf.parseArgs(process.argv, [
    { name: 'buildEngine', flags: '--buildengine' },
])

const settings = wtf.loadSettings()

if(args.buildEngine) build.engine()
else build.project()

process.stdout.write(`${wtf.colors.DIM}${wtf.colors.CYAN}Build done!${wtf.colors.CLEAR}\n\n`)