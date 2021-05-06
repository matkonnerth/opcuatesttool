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
            name: 'localhost',
            host: 'localhost',
            port: 4840
        },
        {
            name: 'unified',
            host: 'localhost',
            port: 48010
        }
    ];

