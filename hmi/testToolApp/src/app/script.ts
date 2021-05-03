export interface Script {
    name: string;
}

export interface GetScriptsResponse {
    ok: boolean;
    response: Script[];
}
