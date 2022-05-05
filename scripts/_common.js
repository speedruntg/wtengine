/**
 * 
 * @author Matthew Evans
 * @module wtfsystems/wtengine-tools
 * @see README.md
 * @copyright MIT see LICENSE.md
 * 
 */

const fs = require('fs')
const shell = require('shelljs')
const inquirer = require('inquirer')

const package = require('../package.json')
const config = require('./_config.json')

const constants = {
    CONFIG_SCRIPT:     `npm run wte-config`,
    SYSCHECK_SCRIPT:   `npm run wte-syscheck`,
    SETTINGS_LOCATION: `${__dirname}/../settings.json`
}

/**
 * Display script title & info.
 * @param {String} scriptName 
 */
const showScriptInfo = (scriptName) => {
    process.stdout.write(`\x1b[37m${package.name} > v${package.version} > \x1b[89m`)
    process.stdout.write(`\x1b[97m${scriptName}\x1b[39m\n`)
    process.stdout.write(`\x1b[36m\x1b[4m${package.url}\x1b[24m\x1b[89m\n`)
    process.stdout.write(`\x1b[0m\n`)
}
exports.showScriptInfo = showScriptInfo

/**
 * Display an error message and exit script.
 * @param {String} message Message to display.
 */
const scriptError = (message) => {
    process.stdout.write(`\x1b[31mError:  ${message}  Exiting...\x1b[0m\n`)
    throw new Error(message)
}
exports.scriptError = scriptError

/**
 * Confirmation prompt (wip)
 * @param {String} message Message to display.
 * @param {boolean} dvalue Default answer (Y - true | N - false)
 * @returns {boolean} True if default answer, else false
 */
const confirmPrompt = async (message, dvalue) => {
    if(dvalue == undefined) dvalue = true
    inquirer.prompt([{
        default: dvalue,
        name: 'conf',
        type: 'confirm',
        message: `\x1b[33m${message}\x1b[0m`
    }]).then(res => { return res.conf })
    process.stdout.write(`\x1b[0m\n\n`)
    return true
}
exports.confirmPrompt = confirmPrompt

/**
 * Verify write access to engine settings file.
 * @param {String} permissions File permissions to check, 'rwx' format.
 * Passing nothing checks if the file simply exists.
 * @returns {boolean} True if permission checks succeded.
 * On fail, displays a script error and exit.
 */
const checkSettings = (permissions) => {
    let checkFlags = []
    if(permissions == undefined) checkFlags.push(fs.constants.F_OK)
    if(permissions.includes("r") || permissions.includes("R")) checkFlags.push(fs.constants.R_OK)
    if(permissions.includes("w") || permissions.includes("W")) checkFlags.push(fs.constants.W_OK)
    if(permissions.includes("x") || permissions.includes("X")) checkFlags.push(fs.constants.X_OK)

    if(checkFlags.length == 0) scriptError(`Unable to check settings file!  No proper tests requested!`)

    checkFlags.forEach(fFlag => {
        fs.access(constants.SETTINGS_LOCATION, fFlag, (err) => { scriptError(err) })
    })
    return true
}
exports.checkSettings = checkSettings

/**
 * Save engine settings.
 * @param {JSON} settings Settings as JSON object.
 * @returns {boolean} True if settings saved.
 * On fail, display error and exit running script.
 */
const saveSettings = (settings) => {
    try {
        fs.writeFileSync(constants.SETTINGS_LOCATION, JSON.stringify(settings))
    } catch (err) {
        scriptError(err)
    }
    return true
}
exports.saveSettings = saveSettings

/**
 * Check for necessary applications.
 * @returns {boolean} True if all apps are found.
 * On not found, displays a script error and exit.
 */
const checkApps = () => {
    process.stdout.write(`Checking for necessary applications...\n`)
    config.checkApps.forEach((appCheck) => {
        if(shell.which(appCheck)) process.stdout.write(`\x1b[32m${appCheck} found.\x1b[0m\n`)
        else scriptError(`${appCheck} not found.`)
    })
    return true
}
exports.checkApps = checkApps

/**
 * WIP
 * @returns 
 */
const runSysCheckScript = () => {
    let res = shell.exec(constants.SYSCHECK_SCRIPT, { async: false })
    if(res.stderr != "") return false
    return true
}
exports.runSysCheckScript = runSysCheckScript

/**
 * WIP
 * @returns 
 */
const runConfigScript = () => {
    let res = shell.exec(constants.CONFIG_SCRIPT, { async: false })
    if(res.stderr != "") return false
    return true
}
exports.runConfigScript = runConfigScript
