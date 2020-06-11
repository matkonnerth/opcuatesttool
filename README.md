# WIP: opcuaTestTool

## Useful commands

### Triggering a new job
curl -H "Content-Type: application/json" --data @../tests/rest/newJob.json http://localhost:9080/jobs

### Get finished jobs
curl http://localhost:9080/jobs?finished

### Get results of job with id 0
curl http://localhost:9080/jobs/0

### some good reads
https://florimond.dev/blog/articles/2018/08/restful-api-design-13-best-practices-to-make-your-users-happy/



