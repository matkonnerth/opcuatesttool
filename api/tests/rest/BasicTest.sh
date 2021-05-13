#!/bin/bash

BASE_URL="http://localhost:9888/"
JOBS="api/jobs"
SCRIPTS="api/scripts"
TARGETS="api/targets"

checkStatusCode () {
   echo "request: $1"
   status_code=$(curl --write-out '%{http_code}' --silent --output /dev/null $1)
   echo "StatusCode: ${status_code}"

   if [[ "$status_code" -ne $2 ]] ; then
      echo "statusCode check failed, expected $2"
      exit 10
   fi
}

#get jobs
request="$BASE_URL$JOBS?from=1000&max=1"
checkStatusCode $request 200

request="$BASE_URL$SCRIPTS"
checkStatusCode $request 200

request="$BASE_URL$TARGETS"
checkStatusCode $request 200
