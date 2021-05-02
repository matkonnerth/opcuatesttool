/*getJobs interface*/
export interface Request {
    name: string;
    script: string;
    serverUri: string;
}

export interface Result {
    id: number;
    statusCode: string;
    totalRuntime_ms: number;
    ts_start: number;
    ts_end: number;
}

export interface FinishedJob {
    request: Request;
    result: Result;
}

export interface FinishedJobsResponse {
    response: FinishedJob[];
    statusCode: boolean;
}

/*
{
data: [
{
request: {
id: 11,
name: "singleRead",
script: "read.chai",
serverUri: "opc.tcp://debian:4840"
},
result: {
statusCode: "Ok",
totalRuntime_ms: 742.851115,
ts_start: 1617117497919334000,
ts_stop: 1617117498662184200
}
},
{},
{},
{},
{},
{},
{},
{},
{},
{}
],
ok: true
}
*/