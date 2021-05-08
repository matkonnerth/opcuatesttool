#!/bin/bash
repoDirectory=$1
repoUrl=$2

rm -rf $repoDirectory
git clone $repoUrl $repoDirectory