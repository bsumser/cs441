#!/bin/bash

git log --reverse --since="6am" --oneline --pretty=format:%s --decorate >> proj_log.org
