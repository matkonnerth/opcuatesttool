# opcuaTestTool

# Useful commands

## Triggering a new job
curl -H "Content-Type: application/json" --data @../tests/rest/newJob.json http://localhost:9080/jobs

## Get finished jobs (in progress)
curl http://localhost:9080/jobs/finished

