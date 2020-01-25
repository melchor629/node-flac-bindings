declare namespace Chai {
    export interface Assert {
        throwsAsync<T>(func: () => Promise<T>, pattern: string | RegExp): Promise<void>;
    }
}
