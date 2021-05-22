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