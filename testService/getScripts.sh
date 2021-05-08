#!/bin/bash
repoDirectory=$1
repoUrl="https://github.com/matkonnerth/opcuaTestToolScripts.git"

rm -rf $repoDirectory
git clone $repoUrl $repoDirectory