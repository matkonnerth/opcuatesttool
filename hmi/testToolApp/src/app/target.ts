export interface Target
{
    name: string;
    host: string;
    port: number;
}

export const targets: Target[] =
    [
        {
            name: 'EMS235888',
            host: '10.11.65.192',
            port: 4840
        },
        {
            name: 'sjutImm',
            host: '10.11.65.196',
            port: 4840
        },
        {
            name: 'sjutRobot',
            host: '10.11.65.197',
            port: 4840
        },
        {
            name: 'localhost',
            host: 'localhost',
            port: 4840
        },
        {
            name: 'unifiedLocalhost',
            host: 'localhost',
            port: 48010
        },
    ];

