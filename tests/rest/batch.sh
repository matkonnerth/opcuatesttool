#!/bin/bash
for i in {1..2000}
do
   curl -H "Content-Type: application/json" --data @newJob.json http://localhost:9080/jobs
done
