#!/usr/bin/env node
// Copyright (C) 2025-2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Check that file contains a valid JSON object

'use strict';

import * as fs from 'fs';

// Get file path from command line argument
const filePath = process.argv[2];

if (!filePath) {
    console.error('Usage: check-json.mjs <path-to-config>');
    process.exit(1);
}

try {
    const data = fs.readFileSync(filePath, 'utf-8');
    JSON.parse(data);
} catch (err) {
    console.error(`Invalid ${filePath}:`, err.message);
    process.exit(1);
}

process.exit(0);
