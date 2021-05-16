#!/bin/bash

BASE_URL="http://localhost:9888/"
JOBS="api/jobs"
SCRIPTS="api/scripts"
TARGETS="api/targets"

#does a http get, return statusCode
get () {
   curl --write-out '%{http_code}' --silent --output /dev/null $1
}

#newJob
post() {
   curl -X POST -H "Content-Type: application/json" -d '{ "name": "basicTest", "serverUri": "opc.tcp://debian:4840", "script": "read.chai"}' write-out '%{http_code}' --silent --output /dev/null $1
}

checkHttpStatusCode () {
   echo "request: $2"
   status_code=$($1 $2)
   echo "StatusCode: ${status_code}"

   if [[ "$status_code" -ne $3 ]] ; then
      echo "statusCode check failed, expected $3"
      exit 10
   fi
}

checkNewJobResponse() {
   echo "request: $2"
   status_code=$($1 $2)
   echo "StatusCode: ${status_code}"

   if echo "$status_code" | jq --exit-status '.ok==true' >/dev/null 
   then
      echo "new job returned true"
      echo "job id "
      echo "$status_code" | jq '.id'
   else
      echo "newJob failed, returned false"
      exit 10
   fi
}

#get
request="$BASE_URL$JOBS?from=1000&max=1"
checkHttpStatusCode get $request 200

request="$BASE_URL$SCRIPTS"
checkHttpStatusCode get $request 200

request="$BASE_URL$TARGETS"
checkHttpStatusCode get $request 200

#new job
request="$BASE_URL$JOBS"
checkNewJobResponse post $request

